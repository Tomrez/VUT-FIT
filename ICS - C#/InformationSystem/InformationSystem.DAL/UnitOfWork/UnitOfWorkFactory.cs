using Microsoft.EntityFrameworkCore;

namespace InformationSystem.DAL.UnitOfWork;

public class UnitOfWorkFactory(IDbContextFactory<InformationSystemDbContext> dbContextFactory) : IUnitOfWorkFactory
{
    public IUnitOfWork Create() => new UnitOfWork(dbContextFactory.CreateDbContext());
}